#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f), m_is_platform(false)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}


// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(acceleration), m_width(width), m_height(height), m_entity_type(EntityType), m_is_platform(false), fuel(100.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Texture coordinates for a full texture (single sprite)
    float tex_coords[] = {
        0.0f, 1.0f, // Bottom left
        1.0f, 1.0f, // Bottom right
        1.0f, 0.0f, // Top right
        0.0f, 1.0f, // Bottom left
        1.0f, 0.0f, // Top right
        0.0f, 0.0f  // Top left
    };

    // Vertex positions to map the texture onto
    float vertices[] = {
        -0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f,
        -0.5f, -0.5f,  0.5f, 0.5f,  -0.5f, 0.5f
    };

    // Bind the texture and set up the vertices
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    // Draw the single sprite
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Clean up
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        if (check_collision(&collidable_entities[i])) {
            if (collidable_entities[i].get_platform_status()) {
                if (m_velocity.y > -2.0f) {
                    set_is_winner();
                    return;
                }
                else {
                    set_is_loser();
                    set_crash_land();
                    return;
                }
            }
            else {
                set_is_loser();
                return;
            }
        }
    }


    if (m_acceleration.x > 0) {
        m_acceleration.x -= 0.05;
        if (m_acceleration.x < 0) {
            m_acceleration.x = 0;
        }
    }
    else if (m_acceleration.x < 0) {
        m_acceleration.x += 0.05;
        if (m_acceleration.x > 0) {
            m_acceleration.x = 0;
        }
    }

    // Limit vertical acceleration to max value
    if (m_acceleration.y > 3.0f) {
        m_acceleration.y = 3.0f; // or whatever your max is
    }
    if (m_acceleration.y < -1.0f) {
        m_acceleration.y = -1.0f; // ensure upward velocity isn't too much
    }
    else {
        m_acceleration.y -= 1.0f;
    }

    if (m_acceleration.x == 0) {
        if (m_velocity.x > 0) {
            m_velocity.x -= 0.05;
            if (m_velocity.x < 0) {
                m_velocity.x = 0;
            }
        }
        else if (m_velocity.x < 0) {
            m_velocity.x += 0.05;
            if (m_velocity.x > 0) {
                m_velocity.x = 0;
            }
        }
    }

    m_velocity += m_acceleration * delta_time;
    m_position += m_velocity * delta_time;


    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);

    // Add this line to apply scaling:
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(m_width, m_height, 1.0f));
}

void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
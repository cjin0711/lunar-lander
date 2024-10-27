#ifndef ENTITY_H
#define ENTITY_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"
enum EntityType { PLATFORM, PLAYER, ENEMY };
enum AIType { WALKER, GUARD };
enum AIState { WALKING, IDLE, ATTACKING };


enum AnimationDirection { LEFT, RIGHT, UP, DOWN };

class Entity
{
private:
    bool m_is_active = true;

    int m_walking[4][4]; // 4x4 array for walking animations


    EntityType m_entity_type;
    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    float     m_speed;
  


    // ————— TEXTURES ————— //
    GLuint    m_texture_id;

    // ————— ANIMATION ————— //
    int m_animation_cols;
    int m_animation_frames,
        m_animation_index,
        m_animation_rows;

    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;

    float m_width = 1.0f,
        m_height = 1.0f;
    // ————— COLLISIONS ————— //
    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;

    // ----- TERMINATION ----- //
    bool m_is_platform = false;
    bool is_winner = false;
    bool is_loser = false;
    bool crash_land = false;

    // ----- FUEL ----- //
    float fuel;
    bool depleted = false;



public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 4;

    // ————— METHODS ————— //
    Entity();
    Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float width, float height, EntityType EntityType); // Simpler constructor
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState); // AI constructor
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;

    void update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count);
    void render(ShaderProgram* program);

    void normalise_movement() { m_movement = glm::normalize(m_movement); }

    void face_left() { m_animation_indices = m_walking[LEFT]; }
    void face_right() { m_animation_indices = m_walking[RIGHT]; }
    void face_up() { m_animation_indices = m_walking[UP]; }
    void face_down() { m_animation_indices = m_walking[DOWN]; }

    void move_left() { m_acceleration.x = -1.5f; face_left(); }
    void move_right() { m_acceleration.x = 1.5f;  face_right(); }
    void move_up() { m_acceleration.y = 2.0f;  face_up(); }
    void move_down() { m_movement.y = -1.0f; face_down(); }


    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type; };

    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    bool      const get_platform_status() const { return m_is_platform;  }
    bool      const get_is_winner() const { return is_winner; }
    bool      const get_is_loser() const { return is_loser; }
    bool      const get_crash_land() const { return crash_land;  }
    bool      const get_depleted() const { return depleted; }
    float     const get_fuel() const { return fuel; }

    float get_width() const { return m_width; }
    float get_height() const { return m_height; }

    void activate() { m_is_active = true; };
    void deactivate() { m_is_active = false; };
    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type) { m_entity_type = new_entity_type; };
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
    void const set_width(float new_width) { m_width = new_width; }
    void const set_height(float new_height) { m_height = new_height; }

    void const setPlatform() { m_is_platform = true; }
    void const set_is_winner() { is_winner = true; }
    void const set_is_loser() { is_loser = true; }
    void const set_crash_land() { crash_land = true; }

    void const set_fuel(float new_fuel) { fuel = new_fuel; }
    void const set_depleted() { depleted = true; }

    // Setter for m_walking
    void set_walking(int walking[4][4])
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                m_walking[i][j] = walking[i][j];
            }
        }
    }
};

#endif // ENTITY_H
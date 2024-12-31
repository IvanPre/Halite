#ifndef GRAPH_H
#define GRAPH_H

#include "raylib.h"
#include "maths.h"

typedef struct{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Color ball_color;
    float radius;
    // uint16_t id;
} Ball;

typedef struct{
    Ball* balls;
    unsigned int len;
    unsigned int max_len;
} BallArray;

typedef struct{
    Ball* origin_ball;
    Ball* affected_ball;
    float strength;
    bool bothways;
} G_Connection;

typedef struct{
    G_Connection* connections;
    unsigned int len;
    unsigned int max_len;
} ConnectionArray;


BallArray allocateBallArray(unsigned int length){
    BallArray b_array;
    b_array.len = 0;
    b_array.max_len = length;
    b_array.balls = (Ball *) calloc(length, sizeof(Ball));
    return b_array;
}

void deleteBallArray(BallArray* b_array){
    if(b_array){
        free(b_array->balls);
        b_array->balls = NULL;
        b_array = NULL;
    }
}

void addBallToArray(BallArray* b_array, Vector2 position, float radius, Color color){
    if(b_array->len >= b_array->max_len){
        printf("Ball Array maximum reached.");
        // TODO RAISE ERROR
        return;
    }
    Ball new_ball = { .position = position, .radius = radius,
                      .velocity = (Vector2){0.0f, 0.0f},
                      .acceleration = (Vector2){0.0f, 0.0f},
                      .ball_color = color };
    b_array->balls[b_array->len] = new_ball;
    b_array->len++;
}

G_Connection createConnection(Ball* origin_ball, Ball* affected_ball, float strength, bool bothways){
    G_Connection new_connection = { .origin_ball = origin_ball, .affected_ball = affected_ball,
                                    .strength = strength, .bothways = bothways };
    return new_connection;
}

ConnectionArray allocateConnectionArray(unsigned int length){
    ConnectionArray c_array;
    c_array.len = 0;
    c_array.max_len = length;
    c_array.connections = (G_Connection *) calloc(length, sizeof(G_Connection));
    return c_array;
}

void deleteConnectionArray(ConnectionArray* c_array){
    if(c_array){
        free(c_array->connections);
        c_array->connections = NULL;
        c_array = NULL;
    }
}

void addConnectionToArray(ConnectionArray* c_array, G_Connection new_connection){
    if(c_array->len >= c_array->max_len){
        printf("Connection Array maximum reached.");
        // TODO RAISE ERROR
        return;
    }
    c_array->connections[c_array->len] = new_connection;
    c_array->len++;
}


Vector2 calculateForces(Ball b1, Ball b2, float fallout_c){
    const float x_diff = b2.position.x - b1.position.x;
    const float y_diff = b2.position.y - b1.position.y;
    const float radius_sum = b1.radius + b2.radius;

    // calculates the distance squared between both positions
    Vector2 direction = (Vector2) { x_diff, y_diff };
    const float dist = sqrt( x_diff * x_diff + y_diff * y_diff );

    // normalizes the direciton vector
    direction.x /= dist;
    direction.y /= dist;

    float force;
    if(dist <= radius_sum) {
        force = -100000.0f;
    } else {
        const float radius_mul = b1.radius * b2.radius;
        const float x = dist - 2.0f * radius_sum;
        // force = x*x*x * exp(-fallout_c * x) * radius_mul;
        force = x * radius_mul;
    }

    // printf("%f\n", force);
    Vector2 resultant = (Vector2) { force * direction.x, force * direction.y };
    return resultant;
}

void updateConnectionPhysics(ConnectionArray con_array){
    for(unsigned int i=0; i < con_array.len; i++){
        G_Connection* c_connection = &con_array.connections[i];
        Ball* o_ball = c_connection->origin_ball;
        Ball* a_ball = c_connection->affected_ball;

        Vector2 resultant_forces = calculateForces(*o_ball, *a_ball, 1/con_array.connections[i].strength);
        resultant_forces.x *= c_connection->strength;
        resultant_forces.y *= c_connection->strength;

        const float a_radius_sqr = a_ball->radius * a_ball->radius;
        a_ball->acceleration.x += resultant_forces.x / a_radius_sqr;
        a_ball->acceleration.y += resultant_forces.y / a_radius_sqr;
        if(c_connection->bothways){
            const float o_radius_sqr = o_ball->radius * o_ball->radius;
            o_ball->acceleration.x -= resultant_forces.x / o_radius_sqr;
            o_ball->acceleration.y -= resultant_forces.y / o_radius_sqr;
        }
    }
}

void updateBallPhysics(BallArray b_array, float dt, float fallout_c, float friction){
    for(unsigned int i=0; i<b_array.len; i++){
        Ball* c_ball = &b_array.balls[i];
        float sqr_radius = c_ball->radius * c_ball->radius;
        
        // updates acceleration
        for(unsigned int j=i+1; j<b_array.len; j++){
            Ball* j_ball = &b_array.balls[j];
            float j_sqr_radius = j_ball->radius * j_ball->radius;
            
            Vector2 resultant_forces = calculateForces(*c_ball, *j_ball, fallout_c);

            // applies the force 
            c_ball->acceleration.x += resultant_forces.x / sqr_radius;
            c_ball->acceleration.y += resultant_forces.y / sqr_radius;

            j_ball->acceleration.x -= resultant_forces.x / j_sqr_radius;
            j_ball->acceleration.y -= resultant_forces.y / j_sqr_radius;
        }

        c_ball->velocity.x *= friction;
        c_ball->velocity.y *= friction;

        // Updates velocity and position
        c_ball->velocity.x += c_ball->acceleration.x * dt;
        c_ball->velocity.y += c_ball->acceleration.y * dt;
        c_ball->position.x += c_ball->velocity.x * dt;
        c_ball->position.y += c_ball->velocity.y * dt;

        // Resets acceleration
        c_ball->acceleration = (Vector2){ 0.0f, 0.0f };
    }
}

void applyScreenBoundaries(BallArray b_array, int screenWidth, int screenHeight){
    for(unsigned int i=0; i<b_array.len; i++){
        Ball* c_ball = &b_array.balls[i];
        float r = c_ball->radius;
        if(c_ball->position.x < r) { c_ball->velocity.x = abs(c_ball->velocity.x); }
        else if(c_ball->position.x > screenWidth-r) { c_ball->velocity.x = -abs(c_ball->velocity.x); }

        if(c_ball->position.y < r) { c_ball->velocity.y = abs(c_ball->velocity.y); }
        else if(c_ball->position.y > screenHeight-r) { c_ball->velocity.y = -abs(c_ball->velocity.y); }
    }
}

void drawBalls(BallArray b_array){
    for(unsigned int i=0; i<b_array.len; i++){
        Ball* c_ball = &b_array.balls[i];
        DrawCircleV(c_ball->position, c_ball->radius, c_ball->ball_color);
    }
}



#endif
/* Auto-generated code - do not edit! */

typedef struct PlayerData {
   int health;
   int level;
   float position;
} PlayerData;

typedef struct WorldData {
   PlayerData player;
   // Enemy enemy;  // Error: Unresolved or invalid type 'Enemy'
} WorldData;

typedef struct EnemyData {
   // int !health;  // Error: Cannot use special characters or numbers in field names.
   float position;
} EnemyData;

// Duplicate object name 'Enemy'
// typedef struct EnemyData {
// } EnemyData;


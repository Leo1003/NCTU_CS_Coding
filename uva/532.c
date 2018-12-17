#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DUNGEON_SIZE 32
#define QUEUE_MAX 10000

#define EAST    0
#define SOUTH   1
#define WEST    2
#define NORTH   3
#define UPPER   4
#define BOTTOM  5

#define cord(_map, _pos) (_map[_pos.x][_pos.y][_pos.z])
/*
 * S Start
 * E End
 * . Air
 * # Block
 * $ Locked
 */
typedef struct {
    int x, y, z;
} pos_t;

typedef struct {
    pos_t data[QUEUE_MAX];
    int front, back;
} queue;

int l, r, c;
char dungeon[DUNGEON_SIZE][DUNGEON_SIZE][DUNGEON_SIZE];
int dis[DUNGEON_SIZE][DUNGEON_SIZE][DUNGEON_SIZE];

queue bfs_queue;

void queue_init(queue *q) {
    q->front = -1;
    q->back = -1;
}

int queue_isempty(queue *q)
{
    return (q->back - q->front == 0);
}

int queue_rotate(queue *q)
{
    fprintf(stderr, "rotate: %d ~ %d\n", q->front, q->back);
    int move = q->front - -1;
    if (!move) {
        return 0;
    }
    int len = q->back - q->front;
    memmove(&q->data[0], &q->data[q->front + 1], sizeof(pos_t) * len);
    
    q->front = -1;
    q->back = -1 + len;
    return move;
}

void queue_enqueue(queue *q, pos_t pos)
{
    if (q->back >= QUEUE_MAX - 1) {
        if (queue_rotate(q) == 0) {
            abort();
        }
    }
    q->back++;
    q->data[q->back] = pos;
}

pos_t queue_dequeue(queue *q)
{
    if (queue_isempty(q)) {
        abort();
    }
    q->front++;
    return q->data[q->front];
}

void drop_line()
{
    while (getchar() != '\n')
        ;
}

pos_t move(pos_t pos, int dir)
{
    switch (dir) {
        case EAST:
            pos.z++;
            break;
        case SOUTH:
            pos.y++;
            break;
        case WEST:
            pos.z--;
            break;
        case NORTH:
            pos.y--;
            break;
        case UPPER:
            pos.x++;
            break;
        case BOTTOM:
            pos.x--;
            break;
    }
    return pos;
}

int check(pos_t pos) {
    if (pos.x < 0 || pos.x >= l || pos.y < 0 || pos.y >= r || pos.z < 0 || pos.z >= c) {
        return 0;
    } 
    switch (cord(dungeon, pos)) {
        case '.':
        case 'E':
            return 1;
        default:
            return 0;
    } 
}

int BFS(pos_t start)
{
    queue_init(&bfs_queue);
    cord(dis, start) = 0;
    queue_enqueue(&bfs_queue, start);

    while (!queue_isempty(&bfs_queue)) {
        pos_t cur = queue_dequeue(&bfs_queue);

        int dir;
        for (dir = 0; dir < 6; dir++) {
            pos_t next = move(cur, dir);
            if (check(next)) {
                cord(dis, next) = cord(dis, cur) + 1;
                if (cord(dungeon, next) == 'E') {
                    return cord(dis, next);
                } else {
                    cord(dungeon, next) = '$';
                }
                queue_enqueue(&bfs_queue, next);
            }
        }
    }

    return -1;
}

int main() {
    while (scanf("%d%d%d", &l, &r, &c)) {
        drop_line();
        if (!l && !r && !c) {
            break;
        }
        /*fprintf(stderr, "[%d, %d, %d]\n", l, r, c);*/
        pos_t start;
        int i, j, k;
        for (i = 0; i < l; i++) {
            /*fprintf(stderr, "Level: %d\n", i);*/
            for (j = 0; j < r; j++) {
                scanf("%s", dungeon[i][j]);
                for (k = 0; k < c; k++) {
                    if (dungeon[i][j][k] == 'S') {
                        start.x = i;
                        start.y = j;
                        start.z = k;
                    }
                }
                drop_line();
            }
            drop_line();
        }
        
        int ans = BFS(start);

        if (ans != -1) {
            printf("Escaped in %d minute(s).\n", ans);
        } else {
            printf("Trapped!\n");
        }
    }
    return 0;
}


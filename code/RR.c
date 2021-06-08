// #include "../datastructures/schedulerQueue.c"
// #include "scheduler.c"
// int fixed_time = 5;

// int main()
// {
//     queue *q = queue_init();
//     int count = 0;
//     int clk = 0;
//     queue_push(q, 5);
//     while (true)
//     {

//         if (queue_empty)
//         {
//             break;
//         }

//         if (pcb[0]->remaining_time == 0)
//         {

//             node *temp = queue_pop(q);
//             pcb[0]->Turn_around_time = clk - pcb[0]->arrival_time;
//             queue_push(q, pcb[1]->remaining_time);
//             count = 0;

//             continue;
//         }

//         if (count == fixed_time)
//         {

//             node *temp = queue_pop(q);
//             if (pcb[temp->data]->remaining_time != 0)
//             {
//                 queue_push(q, pcb[temp->data]->remaining_time);
//             }
//             pcb[temp->data]->remaining_time--;
//             count = 0;
//             continue;
//         }
//         count++;
//     }
// }
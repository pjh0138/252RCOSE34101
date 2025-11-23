#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/slab.h>

typedef struct os2025_node //node 선언
{
    int value;
    struct os2025_node *next;
} NODE;

typedef struct //linked list의 메타 데이터
{
    int count;
    NODE *head;
} linked_list;

static linked_list l_list = {
    .count = 0,
    .head = NULL,
}; //메타 데이터 초기화

SYSCALL_DEFINE1(os2025_add, int, val) //add 함수
{
    NODE *n = kmalloc(sizeof(*n), GFP_KERNEL); //노드 공간 선언
    if (!n)
    {
        return -ENOMEM;
    }

    //linked list의 헤드 다음에 위치시켜주는 코드
    n -> value = val; //노드에 값 집어넣기
    n -> next = l_list.head;
    l_list.head = n;
    l_list.count++;
    
    NODE *pos = l_list.head;
    int trial = l_list.count;

    pr_cont("list head");
    while (pos != NULL && trial > 0)
    {
        pr_cont("-> [%d]", pos->value);
        pos = pos->next;
        trial--;
    }
     pr_cont("-> NULL\n");
    return 0;
}

SYSCALL_DEFINE2(os2025_insert, int, tar_val, int, val)//insert 함수
{
    NODE *cur;
    cur = l_list.head;

    NODE *n = kmalloc(sizeof(*n), GFP_KERNEL);
    if (!n)
    {
        return -ENOMEM;
    }
    n -> value = val; //노드에 값 집어넣기

    if(!cur)
    {
        kfree(n);
        return -ENOENT;
    }

    while(cur)  // cur == NULL이 될 때까지
    {
        if (cur->value == tar_val)
            break;
        cur = cur->next;
    }

    if (!cur)
    {
        kfree(n);
        return -ENOENT;
    }

    n -> next = cur -> next; //주어진 위치에 삽입하기
    cur -> next = n;

    l_list.count++;

    NODE *pos = l_list.head;
    int trial = l_list.count;

    pr_cont("list head");
    while (pos != NULL && trial > 0)
    {
        pr_cont("-> [%d]", pos->value);
        pos = pos->next;
        trial--;
    }
     pr_cont("-> NULL\n");
    return 0;
}

SYSCALL_DEFINE1(os2025_remove, int, val)
{
    NODE *previous = NULL; //linked list가 끊기지 않도록 삭제한 노드의 이전 노드를 기록해주는 포인터
    NODE *cur = l_list.head;

    while (cur && cur -> value != val) //삭제할 대상/위치 탐색
    {
        previous = cur;
        cur = cur -> next;
    }

    if (!cur)
    {
        return -ENOENT;
    }

    if (previous)
    {
        previous -> next = cur -> next;
    }
    else
    {
        l_list.head = cur -> next;
    }

    kfree(cur); //free로 삭제
    l_list.count--;

    NODE *pos = l_list.head;
    int trial = l_list.count;

    pr_cont("list head");
    while (pos != NULL && trial > 0)
    {
        pr_cont("-> [%d]", pos->value);
        pos = pos->next;
        trial--;
    }
     pr_cont("-> NULL\n");
    return 0;
}

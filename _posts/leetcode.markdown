---
title: leetcode
date: 2017-07-02 17:43:10
categories: 
- 面试
- leetcode
---
## 445 Add two numbers 2
Question:
> You are given two non-empty linked lists representing two non-negative integers. The most significant digit comes first and each of their nodes contain a single digit. Add the two numbers and return it as a linked list.

>You may assume the two numbers do not contain any leading zero, except the number 0 itself.

>Follow up:
What if you cannot modify the input lists? In other words, reversing the lists is not allowed.

Example:

Input: (7 -> 2 -> 4 -> 3) + (5 -> 6 -> 4)
Output: 7 -> 8 -> 0 -> 7
***
题目意思:
两个数字链表形式给出,求和
***
代码如下:
答题思路,将两个链表存放到两个数组中去,缺失的高位用0填补,每次计算类似加法器,设置一个进位计数器carry,每次取相应位于进位计数器相加求得相应数值与下一个进位,最后需要对进位进行判断,如果大于0则再加一位。
```C
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    int length1 = 0;
    struct ListNode* p1 = l1;
    while(p1 != NULL) {
        length1++;
        p1 = p1->next;
    }
    
    int length2 = 0;
    struct ListNode* p2 = l2;
    while(p2 != NULL) {
        length2++;
        p2 = p2->next;
    }
    
    int length = length1 > length2 ? length1 : length2;
    int arr1[length];
    int arr2[length];
    for(int i = 0; i < length; i++) {
        arr1[i] = 0;
        arr2[i] = 0;
    }
    
    p1 = l1;
    for(int posi1 = length - length1; posi1 <= length -1; posi1++) {
        arr1[posi1] = p1->val;
        p1 = p1->next;
    }
    
    p2 = l2;
    for(int posi2 = length - length2; posi2 <= length -1; posi2++) {
        arr2[posi2] = p2->val;
        p2 = p2->next;
    }
    
    int carry = 0;
    int plus = 0;
    struct ListNode* ans = NULL;
    for(int i = length;i >= 1; i--) {
        plus = (arr1[i-1] + arr2[i-1] + carry)%10;
        carry = (arr1[i-1] + arr2[i-1] + carry)/10;
        struct ListNode* temp = (struct ListNode*)malloc(sizeof(struct ListNode));
        temp->val = plus;
        temp->next = ans;
        ans = temp;
    }
    
    struct ListNode* head = NULL;
    if(carry != 0) {
        head = (struct ListNode*)malloc(sizeof(struct ListNode));
        head->val = carry;
        head->next = ans;
    }else {
        head = ans;
    }
    return head;
}
```

吐槽下:lettcode为什么不支持int a[3] = {0}的初始化方式,= =,战战兢兢想着默认会是0，结果不是= =
***
看了排行第一的解答= = 先把链表翻转= =然后该干啥干啥 
我简直是个弱智


## 18 4sums


#include "../inc/ush.h"

t_process *mx_create_process(char** args, const char* custom_path)
{
    t_process *new_node = malloc(sizeof(t_process));
    
    char** paths = mx_get_exec_paths(args[0], custom_path, true);
    new_node->node_id = 0;
    new_node->stopped = new_node->completed = false;
    new_node->path = paths[0] ? mx_strdup(paths[0]) : mx_strdup(args[0]);
    new_node->cmd_name = mx_strdup(args[0]);
    mx_del_strarr(&paths);
    
    new_node->next = NULL;
    return new_node;
}

t_process *mx_process_dup(t_process* src, int node_id) {

    t_process* p_dup = malloc(sizeof(t_process));
    p_dup->pid = src->pid;
    p_dup->status = src->status;
    p_dup->sh_modes = src->sh_modes;
    p_dup->path = mx_strdup(src->path);
    p_dup->cmd_name = mx_strdup(src->cmd_name);
    p_dup->cmd_line = mx_strdup(src->cmd_line);
    p_dup->stopped = src->stopped;
    p_dup->completed = src->completed;
    p_dup->node_id = node_id;
    p_dup->next = NULL;
    return p_dup;

}

void mx_dfl_push_back(t_process** list, t_process* new_node) {

    if (list != NULL && *list == NULL) {
        *list = new_node;
        return;
    }

    t_process* last = *list;
    while (last->next != NULL) {
        last = last->next;
    } 

    new_node->next = last->next;
    last->next = new_node;

}

void mx_process_push_back(t_process **list, t_cmd_utils* utils, const char* custom_path) {

    t_process* new_node = mx_create_process(utils->args, custom_path);
    new_node->cmd_line = mx_strdup(utils->cmd_line);
    mx_dfl_push_back(list, new_node);

}

void mx_created_process_push_back(t_process **list, t_process* p) {

    int s_node_id = mx_process_list_size(*list) + 1;
    t_process* new_node = mx_process_dup(p, s_node_id);
    p->node_id = s_node_id;
    mx_dfl_push_back(list, new_node);

}

void mx_clear_process(t_process** p) {

    if (p && *p) {
        mx_strdel(&(*p)->path);
        mx_strdel(&(*p)->cmd_name);
        mx_strdel(&(*p)->cmd_line);
        free(*p);
        *p = NULL;
    }

}

void mx_process_pop_front(t_process **head) {

    if (head == NULL || *head == NULL) return; 

    if ((*head)->next == NULL) {
        mx_clear_process(head);
        *head = NULL;
        return;
    }

    t_process* temp = *head;
    *head = (*head)->next;
    mx_clear_process(&temp);

}

void mx_process_pop_back(t_process **head) {

    if (head == NULL || *head == NULL) return;

    if ((*head)->next == NULL) {
        mx_clear_process(head);
        return;
    }

    t_process* prelast = *head;
    while (prelast->next->next != NULL) {

        prelast = prelast->next;

    }
    mx_clear_process(&prelast->next);
    prelast->next = NULL;

}

void mx_process_pop_index(t_process **list, int index) {

    int size = 0;
    t_process* head = *list;
    while (head != NULL) {
        head = head->next;
        ++size;
    }

    if (index <= 0) {
        mx_process_pop_front(list);
        return;
    } else if (index >= size) {
        mx_process_pop_back(list);
        return;
    }

    t_process* current = *list;
    for (int i = 0; current != NULL && i < (index - 1); ++i) {
        current = current->next;
    }
    t_process* next = current->next->next;
    mx_clear_process(&current->next);
    current->next = next;

}

void mx_clear_process_list(t_process **list)
{
    if (list == NULL || *list == NULL)
        return;

    t_process *node = *list;
    t_process *next = NULL;

    while (node != NULL)
    {
        next = node->next;
        mx_clear_process(&node);
        node = next;
    }
    
    *list = NULL;
}

int mx_process_list_size(t_process* list) {

    if (list == NULL) return 0;

    t_process* head = list;
    int size = 0;
    while (head != NULL) {

        head = head->next;
        ++size;

    }
    return size;

}

t_process* mx_get_process_by_pid(t_process* list, pid_t pid, int* index) {

    t_process* current = list;
    int i = 0;
    while (current) {

        if (index != NULL)
            *index = i++;

        if (current->pid == pid)   
            return current;
        current = current->next;

    }
    return NULL;

}

t_process* mx_top_process(t_process* list, int* index) {
    
    if (list == NULL)
        return NULL;

    int i = 1;
    t_process* current = list;
    while (current->next != NULL) {

        if (index)
            *index = i++;
        current = current->next;

    }
    return current;

}

t_process* mx_get_process_by_nodeid(t_process* list, int node_id, int* index) {

    t_process* current = list;
    int i = 0;
    while (current) {

        if (index)
            *index = i++;
        if (current->node_id == node_id)
            return current;
        current = current->next;

    }
    return NULL;

}

t_process* mx_get_process_by_name(t_process* list, const char* name, int* index) {

    t_process* current = list;
    int i = 0;
    while (current) {

        if (index)
            *index = i++;
        if (mx_strcmp(current->cmd_name, name) == 0)
            return current;
        current = current->next;

    }
    return NULL;

}

void mx_print_process_list(t_process* list) {

    while (list) {

        mx_printstr(list->path);
        mx_printstr(" pid -- ");
        mx_printint(list->pid);
        mx_printstr("\n");
        list = list->next;

    }

}

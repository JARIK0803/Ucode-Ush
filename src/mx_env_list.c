#include "../inc/ush.h"

t_env_var *mx_create_env_var(char* env_var)
{
    t_env_var *new_node = malloc(sizeof(t_env_var));
    new_node->name = mx_get_var_name(env_var);
    new_node->value = mx_get_var_value(env_var);
    new_node->next = NULL;

    return new_node;
}

void mx_overwrite_env_var(t_env_var** env_var, const char* value) {

    mx_strdel(&(*env_var)->value);
    (*env_var)->value = mx_strdup(value);

}

static void dfl_env_push_back(t_env_var** list, t_env_var* new_node) {

    if (list != NULL && *list == NULL) {
        *list = new_node;
        return;
    }

    t_env_var* last = *list;
    while (last->next != NULL) {
        last = last->next;
    } 

    new_node->next = last->next;
    last->next = new_node;


}

void mx_env_push_back(t_env_var **list, char* env_var) {

    t_env_var* new_node = mx_create_env_var(env_var);
    dfl_env_push_back(list, new_node);

}

void mx_env_dup_push_back(t_env_var** list, t_env_var* var) {

    char* var_str = mx_get_var_str(var);
    t_env_var* new_node = mx_create_env_var(var_str);
    dfl_env_push_back(list, new_node);

}

void mx_env_pop_back(t_env_var **head) {

    if (head == NULL || *head == NULL) return;

    if ((*head)->next == NULL) {
        free(*head);
        *head = NULL;
        return;
    }

    t_env_var* prelast = *head;
    while (prelast->next->next != NULL) {

        prelast = prelast->next;

    }
    free(prelast->next);
    prelast->next = NULL;

}

void mx_env_pop_front(t_env_var **head) {

    if (head == NULL || *head == NULL) return; 

    if ((*head)->next == NULL) {
        free(*head);
        *head = NULL;
        return;
    }

    t_env_var* temp = *head;
    *head = (*head)->next;
    free(temp);

}

void mx_env_pop_index(t_env_var **list, int index) {

    if (index < 0) return;

    int size = 0;
    t_env_var* head = *list;
    while (head != NULL) {
        head = head->next;
        ++size;
    }

    if (index <= 0) {
        mx_env_pop_front(list);
        return;
    } else if (index >= size) {
        mx_env_pop_back(list);
        return;
    }

    t_env_var* current = *list;
    for (int i = 0; current != NULL && i < (index - 1); ++i) {
        current = current->next;
    }
    t_env_var* next = current->next->next;
    free(current->next);
    current->next = next;

}


void mx_env_clear_list(t_env_var **list)
{
    if (list == NULL || *list == NULL)
        return;

    t_env_var *node = *list;
    t_env_var *next = NULL;

    while (node != NULL)
    {
        next = node->next;
        mx_strdel(&node->name);
        mx_strdel(&node->value);
        free(node);
        node = next;
    }
    
    *list = NULL;
}

int mx_env_list_size(t_env_var* list) {

    if (list == NULL) return 0;

    t_env_var* head = list;
    int size = 0;
    while (head != NULL) {

        head = head->next;
        ++size;

    }
    return size;

}

t_env_var* mx_env_swap_nodes(t_env_var* node1, t_env_var* node2) {

    t_env_var* tmp = node2->next;
    node2->next = node1;
    node1->next = tmp;
    return node2;

}

t_env_var* mx_find_env_var(t_env_var* list, char* name, int* index) {

    t_env_var* current = list;
    int i = 0;
    while (current) {

        if (index != NULL)
            *index = i++;
        if (mx_strcmp(current->name, name) == 0)   
            return current;
        current = current->next;

    }
    return NULL;

}

void mx_env_sort_list(t_env_var** list, bool (*cmp)(char* a, char* b)) {

    t_env_var** file;
    bool is_swapped;
  
    int count = mx_env_list_size(*list);

    for (int i = 0; i < count; i++) {
  
        file = list;
        is_swapped = false;
  
        for (int j = 0; j < count - i - 1; j++) {
  
            t_env_var* f1 = *file;
            t_env_var* f2 = f1->next;
  
            if (cmp(f1->name, f2->name)) {

                *file = mx_env_swap_nodes(f1, f2);
                is_swapped = true;
            
            }
  
            file = &(*file)->next;
        }
        if (!is_swapped)
            break;

    }

}

char** mx_get_env_array(t_env_var* list) {

    char** env_array = malloc(sizeof(char*) * (mx_env_list_size(list) + 1));
    int i = 0;
    t_env_var* curr_var = list;
    while (curr_var) {

        env_array[i++] = mx_get_var_str(curr_var);
        curr_var = curr_var->next;

    }
    env_array[i] = NULL;
    return env_array;

}

void mx_print_env_list(t_env_var* list, bool is_env_cmd) {

    while (list) {

        mx_printstr(list->name);
        mx_printstr("=");
        if (!is_env_cmd && mx_strcmp(list->value, "") == 0) {
            mx_printstr("\'\'");
        } else {
            mx_printstr(list->value);
        }
        mx_printstr("\n");
        list = list->next;

    }

}

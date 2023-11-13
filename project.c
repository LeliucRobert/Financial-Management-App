#include <gtk/gtk.h>
#include <string.h>


struct obj{
GtkWidget *date , *amount , *type , *description , *startdate , *enddate;
};

struct trans{
char date[25] , description[100] , type[15];
float amount;
};

struct trans transaction[505];
int transaction_number = 0;

int valid_date(const char *a)
{
    if (strlen(a) > 10)
        return 0;
    if ((a[0] >= '4' || a[0] < '0') ||  (a[0] == '3' && a[1] >= '2') || (a[1] > '9' || a[1] < '0') || (a[0] == '0' && a[1] == '0'))
        return 0;
    if (a[2] != '/')
        return 0;
    if ((a[3] >= '2' || a[3] < '0') || (a[3] == '1' && a[4] >= '3' ) || (a[4] > '9' || a[4] < '0') || (a[0] == '0' && a[1] == '0'))
        return 0;
    if (a[5] != '/')
        return 0;
    if ((a[6] > '9' || a[6] < '0') || (a[7] > '9' || a[7] < '0') || (a[8] > '9' || a[8] < '0') || (a[9] > '9' || a[9] < '0'))
        return 0;
    return 1;
}

int valid_amount(const char *a)
{
    for (int i = 0 ; i < strlen(a) ; i++)
        if(a[i] >'9' || a[i] < '0')
            return 0;
    return 1;
}

void show_error (gchar *message)
{
    GtkWidget *error = gtk_message_dialog_new(GTK_WINDOW(NULL),GTK_DIALOG_MODAL , GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s",message);
    gtk_dialog_run(GTK_DIALOG(error));
    gtk_widget_destroy(error);
}


void show_balance(GtkWidget *balance , gpointer data)
{

    float acc_balance = 0;
    for (int i = 1 ; i <= transaction_number ; i++ )
    {
        if (strcmp(transaction[i].type , "Income") == 0)
            acc_balance += transaction[i].amount;
        else
            acc_balance -= transaction[i].amount;
    }
    GtkWidget *showbalance = gtk_message_dialog_new(GTK_WINDOW(NULL) ,GTK_DIALOG_MODAL , GTK_MESSAGE_INFO , GTK_BUTTONS_OK , "Acount balance: $%.2f" , acc_balance);
    gtk_dialog_run(GTK_DIALOG(showbalance));
    gtk_widget_destroy(showbalance);
}

int date_a_before_b(const char *a , const char *b)
{
    int n = strlen(a) - 1;
        for(int i = n ; i >= 0 ; i--)
            if (a[i] < b[i])
                return 1;
            if (strcmp(a , b) == 0)
                return 1;
    return 0;
}

void save_data_to_file(GtkWidget *widget , gpointer data)
{
    FILE*file = fopen("saved_transactions.txt" , "w");
    if (file == NULL){
        show_error("Error in saving data. File 'saved_transactions.txt' does not exist");
        return;
    }
    else
    for (int i = 1; i <= transaction_number; i++) {
        fprintf(file, "Date: %s ,Description: %s,Amount: %.2f,Type: %s\n",transaction[i].date,transaction[i].description , transaction[i].amount, transaction[i].type);
    }
    fclose(file);
}
void load_data_from_file(GtkWidget *widget , gpointer data)
{
    FILE *file = fopen("load_transactions.txt" , "r");
    if (file == NULL){
        show_error("Error in loading data. File 'load_transaction.txt' does not exist");
        return;
    }
    else {
        char date[10], description[100], type[15];
                float amount;

        transaction_number++;
        while (fscanf(file , "%s %s %f %s \n" , date ,description, &amount ,type) == 4){
            strcpy(transaction[transaction_number].date, date);
            strcpy(transaction[transaction_number].description, description);
            transaction[transaction_number].amount = amount;
            strcpy(transaction[transaction_number].type, type);
            transaction_number++;
        }
    }
    transaction_number--;
    fclose(file);
}
void show_report(GtkWidget *viewbutton , gpointer data)
{
    struct obj *objects = (struct obj *)data;
    float sumpoz = 0.0 , sumneg = 0.0;
    const char *x = gtk_entry_get_text(GTK_ENTRY(objects->startdate));
    const char *y = gtk_entry_get_text(GTK_ENTRY(objects->enddate));
    if (strlen(x) == 0 || strlen(y) == 0)
        show_error("You must enter the start and the end date!");
    else if(!valid_date(x) || !valid_date(y))
        show_error("Invalid date");
    else{
        for (int i = 1 ; i <= transaction_number ; i++)
        {
            if(date_a_before_b(transaction[i].date , y) && date_a_before_b(x , transaction[i].date))
                if (strcmp(transaction[i].type,"Income") == 0)
                    sumpoz = sumpoz + transaction[i].amount;
                else
                    sumneg = sumneg - transaction[i].amount;
        }

        GtkWidget *sum1 = gtk_message_dialog_new(GTK_WINDOW(NULL) ,GTK_DIALOG_MODAL , GTK_MESSAGE_INFO , GTK_BUTTONS_OK , "Your incomes for this period are: $%.2f" "\n" "Your expenses for this period are: $%.2f" , sumpoz , sumneg);
        gtk_dialog_run(GTK_DIALOG(sum1));
        gtk_widget_destroy(sum1);

    }
}
void show_previous_trans(GtkWidget *previous_trans , gpointer data)
{
    GtkWidget *window_trans = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window_trans) , "Previous Transactions");

    GtkWidget *textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL , NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC , GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window),textview);
    gtk_container_add(GTK_CONTAINER(window_trans),scrolled_window);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    GtkTextIter iter;
    gtk_text_buffer_get_start_iter(buffer , &iter);

    for(int i = 1 ; i <= transaction_number ; i++)
    {
     char *date_str = g_strconcat("Date: ", transaction[i].date, "\n", NULL);
            char *description_str =g_strconcat("Description: ", transaction[i].description, "\n", NULL);
            char *amount_str = g_strdup_printf("Amount: $%.2f\n", transaction[i].amount);
            char *type_str = g_strconcat("Type: ", transaction[i].type, "\n\n", NULL);

            gtk_text_buffer_insert(buffer, &iter, date_str, -1);
            gtk_text_buffer_insert(buffer, &iter, description_str, -1);
            gtk_text_buffer_insert(buffer, &iter, amount_str, -1);
            gtk_text_buffer_insert(buffer, &iter, type_str, -1);

            g_free(date_str);
            g_free(description_str);
            g_free(amount_str);
            g_free(type_str);

            gtk_text_buffer_get_end_iter(buffer, &iter);


            gtk_text_buffer_insert(buffer, &iter, "\n", -1);
    }
    gtk_widget_show_all(window_trans);
}

void record_transaction(GtkWidget *submit_button , gpointer data)
{
    struct obj *objects = (struct obj *)data;

    const char *x = gtk_entry_get_text(GTK_ENTRY(objects->date));
    const char *y = gtk_entry_get_text(GTK_ENTRY(objects->description));
    const char *z = gtk_entry_get_text(GTK_ENTRY(objects->amount));
    const char *w = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(objects->type));

    if (strlen(x) == 0 || strlen(y) == 0 || strlen(z) == 0 || w == NULL){
        show_error("All fields must be completed!");
    }else if (!valid_date(x)) {
        show_error("Invalid date!");
    }else if(!valid_amount(z)){
        show_error("Invalid amount!");
    }else{
        transaction_number++;
        strcpy(transaction[transaction_number].date, x);;
        strcpy(transaction[transaction_number].description, y);


        transaction[transaction_number].amount = atof(z);
        strcpy(transaction[transaction_number].type, w);
    }

}

int main()
{
    gtk_init(NULL , NULL);

    GtkWidget *label = gtk_label_new (NULL);
    GtkWidget *box;
    GtkWidget *hbox_2entries;
    GtkWidget *hbox_2display;
    GtkWidget *submit = gtk_button_new_with_label("Submit transaction");
    GtkWidget *accountbalance = gtk_button_new_with_label("Calculate account balance");
    GtkWidget *view_button = gtk_button_new_with_label("View report");
    GtkWidget *save_to_file_button = gtk_button_new_with_label("Save data to file'saved_transactions.txt'");
    GtkWidget *load_from_file_button = gtk_button_new_with_label("Load data from file 'load_transactions.txt'");
    GtkWidget *previous_trans = gtk_button_new_with_label("Show previous transactions");
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    struct obj objects;
    objects.date = gtk_entry_new();
    objects.description = gtk_entry_new();
    objects.amount = gtk_entry_new();
    objects.type = gtk_combo_box_text_new();
    objects.startdate = gtk_entry_new();
    objects.enddate = gtk_entry_new();

    gtk_window_set_title(GTK_WINDOW(window) , "Financial Data Managment");
    g_signal_connect (window , "destroy" , G_CALLBACK(gtk_main_quit), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL , 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Date (DD/MM/YYYY):"), FALSE,FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), objects.date, FALSE,FALSE,10);

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Description:"), FALSE,FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), objects.description, FALSE,FALSE, 10);

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Sum amount: "), FALSE,FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box),objects.amount, FALSE,FALSE, 10);

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Type(Income/Expense: "), FALSE,FALSE, 10);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(objects.type) , "Income");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(objects.type) , "Expense");
    gtk_box_pack_start(GTK_BOX(box),objects.type, FALSE,FALSE, 10);

    gtk_box_pack_start(GTK_BOX(box), submit , FALSE,FALSE , 10);
    g_signal_connect(submit , "clicked" ,  G_CALLBACK(record_transaction) , &objects);

    gtk_box_pack_start(GTK_BOX(box) , accountbalance , FALSE , FALSE , 10);
    g_signal_connect(accountbalance , "clicked" , G_CALLBACK(show_balance) , &objects);

    gtk_box_pack_start(GTK_BOX(box) , previous_trans , FALSE , FALSE , 10);
    g_signal_connect(previous_trans , "clicked" , G_CALLBACK(show_previous_trans) , &objects);

    gtk_box_pack_start(GTK_BOX(box) , gtk_label_new("Transactions report") , FALSE , FALSE , 10);

    hbox_2entries = gtk_box_new(GTK_ORIENTATION_HORIZONTAL , 10);
    hbox_2display = gtk_box_new(GTK_ORIENTATION_HORIZONTAL , 10);
    gtk_container_add(GTK_CONTAINER(box) , hbox_2display);
    gtk_container_add(GTK_CONTAINER(box) , hbox_2entries);

    gtk_box_pack_start(GTK_BOX(hbox_2display) , gtk_label_new("Start date (DD/MM/YYYY):")  ,FALSE , FALSE , 10);
    gtk_box_pack_start(GTK_BOX(hbox_2display) , gtk_label_new("End date (DD/MM/YYYY):")  ,FALSE , FALSE , 10);

    gtk_box_pack_start(GTK_BOX(hbox_2entries) , objects.startdate , FALSE , FALSE , 20);
    gtk_box_pack_start(GTK_BOX(hbox_2entries) , objects.enddate , FALSE , FALSE , 20);

    gtk_box_pack_start(GTK_BOX(box) , view_button , FALSE , FALSE , 10);
    g_signal_connect(view_button , "clicked" , G_CALLBACK(show_report) , &objects);

    gtk_box_pack_start(GTK_BOX(box) , load_from_file_button , FALSE , FALSE , 10);
    gtk_box_pack_start(GTK_BOX(box) , save_to_file_button , FALSE , FALSE , 10);

    g_signal_connect(load_from_file_button , "clicked" , G_CALLBACK(load_data_from_file) , "load_transactions.txt");
    g_signal_connect(save_to_file_button , "clicked" , G_CALLBACK(save_data_to_file) , "saved_transactions.txt");
    gtk_widget_show_all(window);
    gtk_main();
    
    
return 0;
}









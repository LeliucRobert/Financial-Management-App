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
    if ((a[0] >= '4' || a[0] < '0') ||  (a[0] == '3' && a[1] >= '2') || (a[1] > '9' || a[1] < '0') || (a[0] == '0' && a[1] == '0')) // check if the day entered by the user is valid
        return 0;
    if (a[2] != '/')
        return 0;
    if ((a[3] >= '2' || a[3] < '0') || (a[3] == '1' && a[4] >= '3' ) || (a[4] > '9' || a[4] < '0') || (a[0] == '0' && a[1] == '0'))  // check if the month entered by the user is valid
        return 0;
    if (a[5] != '/')
        return 0;
    if ((a[6] > '9' || a[6] < '0') || (a[7] > '9' || a[7] < '0') || (a[8] > '9' || a[8] < '0') || (a[9] > '9' || a[9] < '0'))  // check if the year entered by the user is valid
        return 0;
    return 1;
}

int valid_amount(const char *a) // checks if the amount entered is a number
{
    for (int i = 0 ; i < strlen(a) ; i++)
        if((a[i] >'9' || a[i] < '0') && (a[i] != '.'))
            return 0;
    return 1;
}

void show_error (gchar *message) // function to create a new dialog  with the message error
{
    GtkWidget *error = gtk_message_dialog_new(GTK_WINDOW(NULL),GTK_DIALOG_MODAL , GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s",message); 
    gtk_dialog_run(GTK_DIALOG(error));
    gtk_widget_destroy(error);
}


void show_balance(GtkWidget *balance , gpointer data)
{

    float acc_balance = 0;
    for (int i = 1 ; i <= transaction_number ; i++ ) // calculate the account balance
    {
        if (strcmp(transaction[i].type , "Income") == 0) // verify the type of the transaction
            acc_balance += transaction[i].amount;
        else
            acc_balance -= transaction[i].amount;
    }
    GtkWidget *showbalance = gtk_message_dialog_new(GTK_WINDOW(NULL) ,GTK_DIALOG_MODAL , GTK_MESSAGE_INFO , GTK_BUTTONS_OK , "Acount balance: $%.2f" , acc_balance); // creates a dialog which shows the account balance
    gtk_dialog_run(GTK_DIALOG(showbalance)); // shows the dialog
    gtk_widget_destroy(showbalance); // it closes it
}

int date_a_before_b(const char *a , const char *b) // function to check if a date is before another date
{
    int n = strlen(a) - 1; // length of the date
        for(int i = n ; i >= 0 ; i--) // we go from reverse because we want to check the year first
            if (a[i] < b[i])
                return 1;
            if (strcmp(a , b) == 0)
                return 1;
    return 0;
}

void save_data_to_file(GtkWidget *widget , gpointer data) // function to  save data to a file
{
    FILE*file = fopen("saved_transactions.txt" , "w"); // we open the file , 'w' comes from  write
    if (file == NULL){
        show_error("Error in saving data. File 'saved_transactions.txt' does not exist"); // if the file does not exist we print an error
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
    FILE *file = fopen("load_transactions.txt" , "r"); // we open the file , 'r' comes from read
    if (file == NULL){
        show_error("Error in loading data. File 'load_transaction.txt' does not exist"); // if the file does not extist we print an error
        return;
    }
    else {

        transaction_number++;
        while (fscanf(file , "%10s %30s %f %6s \n" , transaction[transaction_number].date ,transaction[transaction_number].description, &transaction[transaction_number].amount ,transaction[transaction_number].type) == 4){ //we read the data from the file, we use %10s so in the first parameter which is the date we can get a maximum of 10 characters
            transaction_number++;
        }
    }
    transaction_number--; // we decrement the transaction number because in the while loop it will be incremented one more after the last read
    fclose(file);
}
void show_report(GtkWidget *viewbutton , gpointer data) // function to show report of expenses and incomes for a certain period of time
{
    struct obj *objects = (struct obj *)data; // gets the data transmitted as parameter
    float sumpoz = 0.0 , sumneg = 0.0;
    const char *x = gtk_entry_get_text(GTK_ENTRY(objects->startdate)); // gets the start date
    const char *y = gtk_entry_get_text(GTK_ENTRY(objects->enddate)); // gets the end date
    if (strlen(x) == 0 || strlen(y) == 0) // if one or both entries are not completed it shows an error
        show_error("You must enter the start and the end date!");
    else if(!valid_date(x) || !valid_date(y))// if the dates are not valid it shows an error
        show_error("Invalid date");
    else{
        for (int i = 1 ; i <= transaction_number ; i++)
        {
            if(date_a_before_b(transaction[i].date , y) && date_a_before_b(x , transaction[i].date)) // we check if the transaction.date we are currently at is before the end_date and after the start_date
                if (strcmp(transaction[i].type,"Income") == 0)// if the type is 'income' we add to a sum that represents the income sum else we substract from another sum that represents the expenses sum
                    sumpoz = sumpoz + transaction[i].amount;
                else
                    sumneg = sumneg - transaction[i].amount;
        }

        GtkWidget *sum1 = gtk_message_dialog_new(GTK_WINDOW(NULL) ,GTK_DIALOG_MODAL , GTK_MESSAGE_INFO , GTK_BUTTONS_OK , "Your incomes for this period are: $%.2f" "\n" "Your expenses for this period are: $%.2f" , sumpoz , sumneg); // we create a new dialog that show the report of incomes and expenses
        gtk_dialog_run(GTK_DIALOG(sum1));
        gtk_widget_destroy(sum1);

    }
}
void show_previous_trans(GtkWidget *previous_trans , gpointer data) // function that shows all the previous transactions
{
    GtkWidget *window_trans = gtk_window_new(GTK_WINDOW_TOPLEVEL); // creates a new window that will be displayed when the button associated with this function will be pressed
    gtk_window_set_title(GTK_WINDOW(window_trans) , "Previous Transactions"); // set the title of the window

    GtkWidget *textview = gtk_text_view_new(); // it creates a text widget
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);// it set it so that it cannot be edited

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL , NULL); // it creates a scrolled window so that when we have a lot of transactions we can scroll through them
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC , GTK_POLICY_AUTOMATIC); // this set the scrolled window widget to show the scroll line just when it is necessary. We can change GTK_POLICY_AUTOMATIC to GTK_POLICY_ALWAYS and it will always show the scroll line. The first parameter is for the horizontal line, and then the second is for the verical line
    gtk_container_add(GTK_CONTAINER(scrolled_window),textview); // We add  the textview as a child of scrolled_window. This means basically that the textview is inside the scrolled window widget
    gtk_container_add(GTK_CONTAINER(window_trans),scrolled_window);// We add the scrolled_window as a child of the window_trans .This means basically that the scrolled_window is inside the window.
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));// it assigns to buffer the addres of the text inside textview widget
    GtkTextIter iter; // declares a variable iter with  which we can traverse the text
    gtk_text_buffer_get_start_iter(buffer , &iter);// assign the iter to the start of the text

    for(int i = 1 ; i <= transaction_number ; i++)// we go through all the transactions
    {
            char *date_str = g_strconcat("Date: ", transaction[i].date, "\n", NULL); // concatenates using a function from GTK library string "Date" with the 'date' of the actual transaction 
            char *description_str =g_strconcat("Description: ", transaction[i].description, "\n", NULL);  // concatenates using a function from GTK library string "Description" with the 'description' of the actual transaction
            char *amount_str = g_strdup_printf("Amount: $%.2f\n", transaction[i].amount); // we use this function from GTK library to format the 'amount'  of the actual transaction with 2 decimals at float point and to concatenate it with "Amount"
            char *type_str = g_strconcat("Type: ", transaction[i].type, "\n\n", NULL); // concatenates using a function from GTK library string "Type" with the 'type' of the actual transaction

            gtk_text_buffer_insert(buffer, &iter, date_str, -1);// we inset the the text from date_str into the buffer
            gtk_text_buffer_insert(buffer, &iter, description_str, -1);// we inset the the text from description_str into the buffer
            gtk_text_buffer_insert(buffer, &iter, amount_str, -1);// we inset the the text from amount_str into the buffer
            gtk_text_buffer_insert(buffer, &iter, type_str, -1);// we inset the the text from type_str into the buffer

            g_free(date_str);
            g_free(description_str);
            g_free(amount_str);
            g_free(type_str);//we free the memory allocated by the functions g_strconcat and g_strdup_printf

            gtk_text_buffer_get_end_iter(buffer, &iter);//we set the iterator at the end of the text


            gtk_text_buffer_insert(buffer, &iter, "\n", -1);// we add a new line
    }
    gtk_widget_show_all(window_trans);
}

void record_transaction(GtkWidget *submit_button , gpointer data)// function to record a transaction
{
    struct obj *objects = (struct obj *)data;//gets the data transmited as parameter. (objects struct)

    const char *x = gtk_entry_get_text(GTK_ENTRY(objects->date));//get the date entered by the user
    const char *y = gtk_entry_get_text(GTK_ENTRY(objects->description));//get the description entered by the user
    const char *z = gtk_entry_get_text(GTK_ENTRY(objects->amount));//get the amount entered by the user
    const char *w = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(objects->type));//get the type of transaction choosen by the user

    if (strlen(x) == 0 || strlen(y) == 0 || strlen(z) == 0 || w == NULL){// verify if all fields are completed
        show_error("All fields must be completed!");
    }else if (!valid_date(x)) {//verify if the date is valid
        show_error("Invalid date!");
    }else if(!valid_amount(z)){//verify if the amount is valid
        show_error("Invalid amount!");
    }else{
        transaction_number++;//increase the transactions counter
        strcpy(transaction[transaction_number].date, x);;//store the date entered by the user
        strcpy(transaction[transaction_number].description, y);//store the description entered by the user


        transaction[transaction_number].amount = atof(z);//store the amount entered by the user as a float number. we use atof to convert a string to a float number
        strcpy(transaction[transaction_number].type, w);//store the type choosen by the user
    }

}

int main()
{
    gtk_init(NULL , NULL);

    GtkWidget *box;
    GtkWidget *hbox_2entries;
    GtkWidget *hbox_2display;
    GtkWidget *submit = gtk_button_new_with_label("Submit transaction");
    GtkWidget *accountbalance = gtk_button_new_with_label("Calculate account balance");
    GtkWidget *view_button = gtk_button_new_with_label("View report");
    GtkWidget *save_to_file_button = gtk_button_new_with_label("Save data to file'saved_transactions.txt'");
    GtkWidget *load_from_file_button = gtk_button_new_with_label("Load data from file 'load_transactions.txt'");
    GtkWidget *previous_trans = gtk_button_new_with_label("Show previous transactions"); 
    // create buttons
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//create the main window
    struct obj objects;
    objects.date = gtk_entry_new();
    objects.description = gtk_entry_new();
    objects.amount = gtk_entry_new();
    objects.type = gtk_combo_box_text_new();
    objects.startdate = gtk_entry_new();
    objects.enddate = gtk_entry_new();
    //create the entries(the place where the user types the informations)

    gtk_window_set_title(GTK_WINDOW(window) , "Financial Data Managment");//set the title of the main window
    g_signal_connect (window , "destroy" , G_CALLBACK(gtk_main_quit), NULL);//closes the window when you press the close button

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL , 10);//create a new box within we will put all the buttons, labels, and entries
    gtk_container_add(GTK_CONTAINER(window), box);// add the box as a child of main window (the box is inside the main window)

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Date (DD/MM/YYYY):"), FALSE,FALSE, 10); // add a label
    gtk_box_pack_start(GTK_BOX(box), objects.date, FALSE,FALSE,10);//add the entry of date in box

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Description:"), FALSE,FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), objects.description, FALSE,FALSE, 10);// add the entry for description in box

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Sum amount: "), FALSE,FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box),objects.amount, FALSE,FALSE, 10);// add the entry for amount in box

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Type(Income/Expense: "), FALSE,FALSE, 10);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(objects.type) , "Income");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(objects.type) , "Expense");//creates a combo box (the user can choose between these two options)
    gtk_box_pack_start(GTK_BOX(box),objects.type, FALSE,FALSE, 10);//add the combo box in the box

    gtk_box_pack_start(GTK_BOX(box), submit , FALSE,FALSE , 10);//add the submit button
    g_signal_connect(submit , "clicked" ,  G_CALLBACK(record_transaction) , &objects);//if the submit button is pressed it executes the function record_transaction with the parameter: ojbects

    gtk_box_pack_start(GTK_BOX(box) , accountbalance , FALSE , FALSE , 10);//add the account balance button
    g_signal_connect(accountbalance , "clicked" , G_CALLBACK(show_balance) , &objects);//if the submit button is pressed it executes the function show_balance with the parameter: ojbects

    gtk_box_pack_start(GTK_BOX(box) , previous_trans , FALSE , FALSE , 10);//add the button previous_trans
    g_signal_connect(previous_trans , "clicked" , G_CALLBACK(show_previous_trans) , &objects);//if the submit button is pressed it executes the function show_previous_trans with the parameter: ojbects

    gtk_box_pack_start(GTK_BOX(box) , gtk_label_new("Transactions report") , FALSE , FALSE , 10);

    hbox_2entries = gtk_box_new(GTK_ORIENTATION_HORIZONTAL , 10);// add a new horizontal box where we will add the entries for the start and end date
    hbox_2display = gtk_box_new(GTK_ORIENTATION_HORIZONTAL , 10);// add a new horizontal box where we will add the labels for the start and end date
    gtk_container_add(GTK_CONTAINER(box) , hbox_2display);//add the hbox_2display as a child of the box (the hbox_2display will be inside the box)
    gtk_container_add(GTK_CONTAINER(box) , hbox_2entries);//add the hbox_2entries as a child of the box (the hbox_2entries will be inside the box)

    gtk_box_pack_start(GTK_BOX(hbox_2display) , gtk_label_new("Start date (DD/MM/YYYY):")  ,FALSE , FALSE , 10);//adds in the new hbox_2display the labels
    gtk_box_pack_start(GTK_BOX(hbox_2display) , gtk_label_new("End date (DD/MM/YYYY):")  ,FALSE , FALSE , 10);

    gtk_box_pack_start(GTK_BOX(hbox_2entries) , objects.startdate , FALSE , FALSE , 20);//adds in the new hbox_2entries the entries
    gtk_box_pack_start(GTK_BOX(hbox_2entries) , objects.enddate , FALSE , FALSE , 20);

    gtk_box_pack_start(GTK_BOX(box) , view_button , FALSE , FALSE , 10);//add a view button in the box
    g_signal_connect(view_button , "clicked" , G_CALLBACK(show_report) , &objects);//executes the function show_report when the button view_button is pressed and it sends as parameter &objects

    gtk_box_pack_start(GTK_BOX(box) , load_from_file_button , FALSE , FALSE , 10);
    gtk_box_pack_start(GTK_BOX(box) , save_to_file_button , FALSE , FALSE , 10);//adds buttons in box

    g_signal_connect(load_from_file_button , "clicked" , G_CALLBACK(load_data_from_file) , "load_transactions.txt");
    g_signal_connect(save_to_file_button , "clicked" , G_CALLBACK(save_data_to_file) , "saved_transactions.txt"); //connects the buttons to the functions with the parameters the name of the files
    gtk_widget_show_all(window);//shows the window
    gtk_main();
    
    
return 0;
}











int agendar_polling (void (* callback) () ) {
    if (next_callback == MAX_CALLBACKS) {return -1;}
    callbacks[next_callback] = callback;
    next_callback ++;
    return next_callback-1;
}


boolean desagendar_polling (byte num_callback) {
    
    if (num_callback >= next_callback) {return false;}
    
    //// borrar
    
    next_callback --;
 
}

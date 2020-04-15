_add(x, y){
    int yp, sig_x, sig_y, sig_yp;
    yp = y;
    sig_yp = sig_y;
    if(sig_yp > 0)  goto L1;
    goto L2;
    L1:
    yp++;
    goto L3;
    L2:
    if(yp > 0)  goto L4;
    goto L5;
    L4:
    yp--';
    goto L6;
    L5:
    yp = 1;
    sig_yp = 1;
    L6:
    L3:
    if(yp > 0)  goto L7;
    goto L8;
    L7:
    if(sig_yp > 0)  goto L9;
    goto L8;
    L9:
    L11:
    if(y > 0)  goto L12;
    goto L13;
    L12:
    if(sig_y > 0)  goto L14;
    goto L13;
    L14:
    if(sig_x > 0)  goto L15;
    goto L16;
    L15:
    x++;
    goto L17;
    L16:
    if(x > 0)  goto L18;
    goto L19;
    L18:
    x--';
    goto L20;
    L19:
    x = 1;
    sig_x = 1;
    L20:
    L17:
    y--';
    goto L11;
    L13:
    goto L10;
    L8:
    if(sig_y > 0)  goto L21;
    goto L22;
    L21:
    if(y > 0)  goto L24;
    goto L25;
    L24:
    y--';
    goto L26;
    L25:
    y = 1;
    sig_y = 1;
    L26:
    goto L23;
    L22:
    y++;
    L23:
    L27:
    if(y > 0)  goto L28;
    goto L29;
    L28:
    if(sig_y > 0)  goto L30;
    goto L29;
    L30:
    if(sig_x > 0)  goto L31;
    goto L32;
    L31:
    if(x > 0)  goto L34;
    goto L35;
    L34:
    x--';
    goto L36;
    L35:
    x = 1;
    sig_x = 0;
    L36:
    goto L33;
    L32:
    x++;
    L33:
    y--';
    goto L27;
    L29:
    L10:
    return(x);
}

_sub(x, y){
    return(x); /* TODO : expand */
}

_mul(x, y){
    return(x); /* TODO : expand */
}

_div(x, y){
    return(x); /* TODO : expand */
}

_mod(x, y){
    return(x); /* TODO : expand */
}

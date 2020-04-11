_add(v1, v2)
{
    int v3, v4, v5, v6;
    if(v5 > 0)  goto L1;
    goto L2;
    L1:
    v2++;
    goto L3;
    L2:
    if(v2 > 0) goto L4;
    goto L5;
    L4:
    v2--';
    goto L6;
    L5:
    v2 = 1;
    v5 = 1;
    L6:
    L3:
    if(v2 > 0)  goto L7;
    goto L8;
    L7:
    if(v5 > 0)  goto L9;
    goto L8;
    L9:
    v3 = v2;
    v6 = v5;
    L11:
    if(v3 > 0)  goto L13;
    goto L12;
    L13:
    if(v6 > 0)  goto L14;
    goto L12;
    L14:
    if(v4 > 0)  goto L15;
    goto L16;
    L15:
    v1++;
    goto L17;
    L16:
    if(v1 > 0) goto L18;
    goto L19;
    L18:
    v1--';
    goto L20;
    L19:
    v1 = 1;
    v4 = 1;
    L20:
    L17:
    if(v6 > 0)  goto L23;
    goto L21;
    L23:
    if(v3 > 0)  goto L26;
    goto L24;
    L26:
    v3--';
    goto L25;
    L24:
    v3 = 1;
    v6 = 0;
    L25:
    goto L22;
    L21:
    v3++;
    L22:
    goto L11;
    L12:
    goto L10;
    L8:
    if(v5 > 0)  goto L29;
    goto L27;
    L29:
    v5 = 0;
    goto L28;
    L27:
    v5 = 1;
    L28:
    L30:
    if(v2 > 0)  goto L32;
    goto L31;
    L32:
    if(v5 > 0)  goto L33;
    goto L31;
    L33:
    v1--';
    if(v5 > 0)  goto L36;
    goto L34;
    L36:
    if(v2 > 0)  goto L39;
    goto L37;
    L39:
    v2--';
    goto L38;
    L37:
    v2 = 1;
    v5 = 0;
    L38:
    goto L35;
    L34:
    v2++;
    L35:
    goto L30;
    L31:

    L10:
    return(v1);
}

_sub(x, y){
    return(1);/*TODO: expand*/
}
_mul(x, y){
    return(2);/*TODO: expand*/
}
_div(x, y){
    return(3);/*TODO: expand*/
}
_mod(x, y){
    return(4);/*TODO: expand*/
}

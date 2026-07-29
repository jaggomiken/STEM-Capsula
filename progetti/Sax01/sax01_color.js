function rgba(color) {
    return "rgba("
        + (color.r % 256) + ","
        + (color.g % 256) + ","
        + (color.b % 256) + ","
        + color.a 
        + ")";
}

function hsla(color) {
    return "hsla("
        + (color.h % 361) + ","  // tinta
        + (color.s % 101) + "%," // saturazione
        + (color.l % 101) + "%," // luminosità
        + color.a                // trasparenza
        + ")";
}
/* PEdestrian Light CONtrolled (PELICAN) crossing active object */
typedef struct {
    QActive super;       /* inherits QActive */
    uint8_t pedFlashCtr; /* pedestrian flash counter */
} Pelican;

/* the ctor */
void Pelican_ctor(Pelican * const me, uint8_t timeout) {
    /* call the superclass' ctor... */
    QActive_ctor(&me->super, Q_STATE_CAST(&Pelican_initial));
    . . .    /* initialize the added attributes */
}
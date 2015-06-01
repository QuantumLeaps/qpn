/* PEdestrian Light CONtrolled (PELICAN) crossing active object */
typedef struct {
    QMActive super;      /* inherits QMActive */
    uint8_t pedFlashCtr; /* pedestrian flash counter */
} Pelican;

/* the ctor */
void Pelican_ctor(Pelican * const me, uint8_t timeout) {
    /* call the superclass' ctor... */
    QMActive_ctor(&me->super, Q_STATE_CAST(&Pelican_initial));
    . . .    /* initialize the added attributes */
}
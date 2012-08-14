typedef struct PelicanTag {
    QActive super;                                  /* derived from QActive */
    uint8_t pedFlashCtr;                        /* pedestrian flash counter */
} Pelican;

void Pelican_ctor(Pelican *me, uint8_t timeout);                /* the ctor */

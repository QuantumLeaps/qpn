typedef struct PelicanTag {
    QActive super;         /* inherits QActive */
    uint8_t pedFlashCtr;   /* pedestrian flash counter */
} Pelican;

void Pelican_ctor(Pelican * const me, uint8_t timeout); /* the ctor */

#ifdef QK_ON_CONTEXT_SW
/* NOTE: the context-switch callback is called with interrupts DISABLED */
void QK_onContextSw(uint_fast8_t prev, uint_fast8_t next) {
    (void)prev;
    if (next != (uint_fast8_t)0) {
        //_impure_ptr = &reentrant[next];
    }
}
#endif /* QK_ON_CONTEXT_SW */

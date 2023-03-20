#ifndef __L2_H__
#define __L2_H__


#define IS_ACTIVE_VERTEX(a)       ((a == 1) ? 1 : 0)



/* source vertex property process */
inline prop_t preprocessProperty(prop_t srcProp)
{
  return ((srcProp));
}

/* source vertex property & edge property */
inline prop_t scatterFunc(prop_t srcProp, prop_t edgeProp)
{
  return (srcProp*edgeProp);
}

/* destination property update dst buffer update */
inline prop_t gatherFunc(prop_t ori, prop_t update)
{
  return (((((ori) < (update)) && (update != 0)) || (ori == 0)) ? (update) : (ori));
}

inline prop_t applyFunc( prop_t tProp,
                         prop_t source,
                         prop_t outDeg,
                         unsigned int (&extra)[APPLY_REF_ARRAY_SIZE],
                         unsigned int &active,
                         unsigned int arg
                       )
{
  prop_t update = 0;

  prop_t uProp  = source;
  prop_t wProp;
  if ((tProp == 1) && (uProp == 0))
  {
    extra[0] = 1;
    active = 1;
    wProp = tProp; // current active vertex, not travsered
  }
  else
  {
    extra[0] = 0;
    active = 0;
    wProp = uProp; // not travsered
  }
  update = wProp;

  return update;
}

#endif /* __L2_H__ */

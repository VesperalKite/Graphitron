#ifndef __L2_H__
#define __L2_H__


#define kDamp   ((float) 0.85) 
#define kDampFixPoint   (108) 

inline prop_t preprocessProperty(prop_t srcProp) 
{
  return (srcProp);
}

inline prop_t scatterFunc(prop_t srcProp, prop_t edgeProp) 
{
  return (srcProp);
}

inline prop_t gatherFunc(prop_t ori, prop_t update) 
{
  update = (ori + update);
  return (update);
}

inline prop_t applyFunc(prop_t tProp, prop_t source, prop_t outDeg, unsigned int (&extra)[APPLY_REF_ARRAY_SIZE], unsigned int arg) 
{
  prop_t old_score = source;
  prop_t new_score = (arg + ((kDampFixPoint * tProp) >> (7) ));
  prop_t tmp ;
  if ((outDeg) != ((0) ))
   { 
    tmp = (((1)  << (16) ) / outDeg);
   } 
  else
   { 
    tmp = (0) ;

   } 
  prop_t update = ((new_score * tmp) >> (16) );
  if (((new_score - old_score)) > ((0) ))
   { 
    extra[(0) ] = (new_score - old_score);
   } 
  else
   { 
    extra[(0) ] = (old_score - new_score);

   } 
  return (update);
}


#endif /* __L2_H__ */
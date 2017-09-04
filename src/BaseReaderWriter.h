/*
 * BaseReaderWriter.h
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#ifndef BASEREADERWRITER_H_
#define BASEREADERWRITER_H_
#include <vector>
#include <memory>

typedef std::shared_ptr<std::vector<char> > pt_buffer;

class BaseReaderWriter
{
public:
  virtual pt_buffer Read ()=0;
  virtual void Write (pt_buffer buf)=0;
  virtual ~BaseReaderWriter (void){};
};




#endif /* BASEREADERWRITER_H_ */

#include<serializacion/estructuras.h>

void crearBuffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crearPaquete(t_cod_op cod_op)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = cod_op;
	crearBuffer(paquete);
	return paquete;
}


t_paquete* armarPaqueteCon(void* estructura,t_cod_op cod_op){

	t_paquete* paquete = crearPaquete(cod_op);
	paquete->buffer->size = tamanioEstructura(estructura,cod_op);
	paquete->buffer->stream = serializarEstructura(estructura,paquete->buffer->size,cod_op);

	//printf("Paquete %d creado \n", paquete->codigoOperacion);
	return  paquete;

}

void* serializarPaquete(t_paquete* paquete, uint32_t bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, uint32_t tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(uint32_t));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(uint32_t), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(uint32_t);
}

void enviarPaquete(t_paquete* paquete, int socket_cliente)
{
	uint32_t bytes = paquete->buffer->size + 2*sizeof(uint32_t);
	void* a_enviar = serializarPaquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);
	free(a_enviar);
}

void eliminarPaquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_proceso * crearProceso(uint32_t tamanioProceso, uint32_t sizeInstrucciones, t_instruccion * instrucciones){
	t_proceso * proceso = malloc(sizeof(t_proceso));
	proceso->tamanioProceso = tamanioProceso;
	proceso->sizeInstrucciones = sizeInstrucciones;
	proceso->instrucciones = instrucciones;
	return proceso;
}
void * serializarProceso(void* stream, void* estructura){
  t_proceso* proceso = (t_proceso*) estructura;
  int offset = 0;
  memcpy(stream + offset, &proceso->tamanioProceso,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &proceso->sizeInstrucciones,sizeof(uint32_t));
	serializarInstrucciones(stream + offset, (void*)proceso->instrucciones);
	return stream;
}

t_proceso * deserializarProceso(void* stream){
	t_proceso* proceso = malloc(sizeof(t_proceso));
	//int offset = 0;
	memcpy(&(proceso->tamanioProceso), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(proceso->sizeInstrucciones), stream, sizeof(uint32_t));
	proceso->instrucciones = deserializarInstrucciones(stream);
	return proceso;
}

void * serializarInstrucciones(void* stream, void* estructura){
	int offset = 0;
	uint32_t sizeInstrucciones;
	memcpy(&(sizeInstrucciones), stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	t_instruccion* instrucciones = (t_instruccion*) estructura;

	for(int i=0; i<sizeInstrucciones; i++){
		memcpy(stream + offset, &((instrucciones + i)->identificador), sizeof(instruccion_id));
		offset += sizeof(uint32_t);
		memcpy(stream + offset, &((instrucciones + i)->parametro1), sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset, &((instrucciones + i)->parametro2), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	return stream;
}
t_instruccion * deserializarInstrucciones(void * stream){
	uint32_t sizeInstrucciones;
	memcpy(&(sizeInstrucciones), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	t_instruccion* instrucciones = malloc(sizeof(t_instruccion)*sizeInstrucciones);

	for(int i=0; i<sizeInstrucciones; i++){
		memcpy(&((instrucciones + i)->identificador), stream, sizeof(instruccion_id));
		stream += sizeof(instruccion_id);
		memcpy(&((instrucciones + i)->parametro1), stream, sizeof(uint32_t));
		stream += sizeof(uint32_t);
		memcpy(&((instrucciones + i)->parametro2), stream, sizeof(uint32_t));
		stream += sizeof(uint32_t);
	}
	return instrucciones;
}


void *  serializarMensaje(void* stream, void* estructura){
	t_mensaje* mensaje = (t_mensaje*) estructura;
    
	int offset = 0;
	memcpy(stream + offset, &(mensaje->longitud),sizeof(uint32_t));
	offset += sizeof(uint32_t);
    memcpy(stream + offset, mensaje->texto,mensaje->longitud);
	return stream;
}
t_mensaje *  deserializarMensaje(void* stream){
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));
    int offset = 0;
	memcpy(&(mensaje->longitud), stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	mensaje->texto = malloc(sizeof(char)*(mensaje->longitud));
    memcpy(mensaje->texto, stream + offset, mensaje->longitud);
	return mensaje;
}
void * serializarPCB(void* stream, void* estructura, int offset){
	t_pcb* pcb = (t_pcb*) stream;
	memcpy(stream + offset, &(pcb->id),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(pcb->tamanioProceso),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(pcb->sizeInstrucciones),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(pcb->tablaDePaginas),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(pcb->estimacionRafaga),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	serializarInstrucciones(stream + offset, (void*)pcb->instrucciones);
	return stream;
}
t_pcb * deserializarPCB(void* stream, int offset){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	memcpy(&(pcb->id),stream + offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(pcb->tamanioProceso),stream + offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(pcb->sizeInstrucciones), stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(pcb->tablaDePaginas), stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(pcb->estimacionRafaga), stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(pcb->sizeInstrucciones), stream + offset, sizeof(uint32_t));
	pcb->instrucciones = deserializarInstrucciones(stream+offset);
	
	return pcb;
}
void *  serializarIO(void* stream, void* estructura){
	t_IO* IO = (t_IO*) estructura;
    
	int offset = 0;
	memcpy(stream + offset, &(IO->tiempoBloqueo),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	serializarPCB(stream, estructura, offset);
	return stream;
}
t_mensaje *  deserializarIO(void* stream){
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));
    int offset = 0;
	memcpy(&(mensaje->longitud), stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	mensaje->texto = malloc(sizeof(char)*(mensaje->longitud));
    memcpy(mensaje->texto, stream + offset, mensaje->longitud);
	return mensaje;
}

void * serializarTraduccionDirecciones(void* stream, void* estructura){
	t_traduccion_direcciones* traduccion_direcciones = (t_traduccion_direcciones*) stream;
    int offset = 0;
    memcpy(stream + offset, &(traduccion_direcciones->tamanio_pagina),sizeof(uint32_t));
	offset = sizeof(uint32_t);
    memcpy(stream + offset, &(traduccion_direcciones->paginas_por_tabla),sizeof(uint32_t));
	return stream;
}

t_traduccion_direcciones * deserializarTraduccionDirecciones(void* stream){
	t_traduccion_direcciones* traduccion_direcciones = malloc(sizeof(t_traduccion_direcciones));
    int offset = 0;
    memcpy(&(traduccion_direcciones->tamanio_pagina), stream + offset,sizeof(uint32_t));
	offset = sizeof(uint32_t);
	memcpy(&(traduccion_direcciones->paginas_por_tabla), stream + offset,sizeof(uint32_t));
	return traduccion_direcciones;
}

void* serializarEstructura(void* estructura,int tamanio,t_cod_op cod_op){

	void* stream = malloc(tamanio);

	switch(cod_op){
		case PROCESO:{
			return serializarProceso(stream,estructura);
			break;
		}

		case REQ_TRADUCCION_DIRECCIONES_CPU_MEMORIA:{
			return serializarMensaje(stream,estructura);

			break;
		}
		case RES_TRADUCCION_DIRECCIONES_MEMORIA_CPU:{
			return serializarTraduccionDirecciones(stream,estructura);
			break;
		}
		case REQ_DATO_DIRECCION_LOGICA_CPU_MEMORIA:{
			return serializarMensaje(stream,estructura);
			break;
		}
		case RES_DATO_DIRECCION_LOGICA_MEMORIA_CPU:{
			return serializarMensaje(stream,estructura);
			break;
		}
		case REQ_PCB_A_EJECUTAR_KERNEL_CPU:{
			return serializarPCB(stream,estructura,0);
			break;
		}
		case PCB_EJECUTADO_IO_CPU_KERNEL:{
			return serializarIO(stream,estructura);
			break;
		}
		case PCB_EJECUTADO_EXIT_CPU_KERNEL:{
			return serializarPCB(stream,estructura,0);
		}
		case PCB_EJECUTADO_INTERRUPCION_CPU_KERNEL:{
			return serializarPCB(stream,estructura,0);
		}
		default:
			fprintf(stderr,"Código de operacion %d no contemplado", cod_op);
			exit(EXIT_FAILURE);
	}

}

int tamanioEstructura(void* estructura ,t_cod_op cod_op){

	switch(cod_op){

		case PROCESO:{
			t_proceso * proceso = (t_proceso *) estructura;
			return sizeof(uint32_t)*2 + proceso->sizeInstrucciones*(sizeof(uint32_t)*2 + sizeof(instruccion_id));
			break;
		}
		case REQ_TRADUCCION_DIRECCIONES_CPU_MEMORIA:{
			t_mensaje * msg = (t_mensaje*) estructura;
			return msg->longitud + sizeof(uint32_t);
			break;
		}
		case REQ_DATO_DIRECCION_LOGICA_CPU_MEMORIA:{
			t_mensaje * msg = (t_mensaje*) estructura;
			return msg->longitud + sizeof(uint32_t);
			break;
		}
		case RES_DATO_DIRECCION_LOGICA_MEMORIA_CPU:{
			t_mensaje * msg = (t_mensaje*) estructura;
			return msg->longitud + sizeof(uint32_t);
			break;
		}
		case RES_TRADUCCION_DIRECCIONES_MEMORIA_CPU:{
			return sizeof(uint32_t);
			break;
		}
		case REQ_PCB_A_EJECUTAR_KERNEL_CPU:{
			t_pcb * pcb = (t_pcb *) estructura; 
			return sizeof(uint32_t)*6 + pcb->sizeInstrucciones*(sizeof(uint32_t)*2 + sizeof(instruccion_id));
		}
		case PCB_EJECUTADO_IO_CPU_KERNEL:{
			return sizeof(t_pcb) + sizeof(uint32_t);
			break;
		}
		case PCB_EJECUTADO_EXIT_CPU_KERNEL:{
			return sizeof(t_pcb);
			break;
		}
		case PCB_EJECUTADO_INTERRUPCION_CPU_KERNEL:{
			return sizeof(t_pcb);
			break;
		}
		
		default:
			fprintf(stderr,"Código de operacion %d no contemplado", cod_op);
			exit(EXIT_FAILURE);
	}

}


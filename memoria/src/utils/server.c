#include <utils/server.h>

void deserializarSegun(t_paquete* paquete, int socket){

	switch(paquete->codigo_operacion){
		case REQ_TRADUCCION_DIRECCIONES_CPU_MEMORIA:{
            log_info(logger, "se solicita traducciones de direciones");
            t_traduccion_direcciones * traduccionDirecciones = malloc(sizeof(t_traduccion_direcciones));
            
            traduccionDirecciones->tamanio_pagina = TAM_PAGINA;
            traduccionDirecciones->paginas_por_tabla = ENTRADAS_POR_TABLA;
            t_paquete* paquete = armarPaqueteCon(traduccionDirecciones, RES_TRADUCCION_DIRECCIONES_MEMORIA_CPU);
            enviarPaquete(paquete,socket);
            log_info(logger, "se envia traducciones de direciones");
			break;
        }
        case REQ_CREAR_PROCESO_KERNEL_MEMORIA:{
            log_info(logger, "se solicita crear proceso");
            uint32_t tablaPaginas1erNivel;
            t_pcb * pcb = deserializarPCB(paquete->buffer->stream, 0);
            inicializarEstructurasProceso(pcb->tamanioProceso);
            t_paquete* paqueteRespuesta = armarPaqueteCon(tablaPaginas1erNivel, RES_CREAR_PROCESO_KERNEL_MEMORIA);
            enviarPaquete(paqueteRespuesta,socket);
            eliminarPaquete(paqueteRespuesta);
            log_info(logger, "se envia tabla Paginas 1er Nivel");
			break;
        }
        case REQ_FIN_PROCESO_KERNEL_MEMORIA:{
            uint32_t * id = deserializarUINT32_T(paquete->buffer->stream);
            log_info(logger, "se solicita borrar memora y swap del proceso: %d", *id);
        }
		default:{
            break;
        }
			
    }
	eliminarPaquete(paquete);
	close(socket);
}

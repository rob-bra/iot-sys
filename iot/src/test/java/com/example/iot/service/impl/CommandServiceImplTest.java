package com.example.iot.service.impl;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.dto.PendingCommandResponse;
import com.example.iot.repository.CommandRepository;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.time.Instant;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.mockito.Mockito.*;

class CommandServiceImplTest {

	// attributo della classe CommandServiceImpl
    private CommandRepository commandRepository;
    // istanza vera della classe da testare
    private CommandServiceImpl commandService;

    /* inizializzo mock e istanza vera */
    @BeforeEach
    void setUp() {
        commandRepository = mock(CommandRepository.class);
        commandService = new CommandServiceImpl(commandRepository);
    }

    /**/
    @Test
    void createCommand_shouldDelegateRepository() {
    	/* esempio di richiesta */
        CommandRequest request = new CommandRequest();
        request.setType("FORCE_MEASUREMENT");
        request.setPayload("immediate");
        
        // invocazione della richiesta alla repository mock
        commandService.createCommand("DEV001", request);
        
        /* NON VA BENE - gli istanti di tempo saranno diversi anche se molto vicini */
//        String createdAt = Instant.now().toString();
//        verify(commandRepository, times(1)).createCommand("DEV001", request, createdAt);
        
        /* Aggiro il problema utilizzando apposite funzioni: eq() e anyString() */
        verify(commandRepository, times(1)).createCommand(eq("DEV001"), eq(request), anyString());
    }

    @Test
    void getCommandsByDeviceId_shouldReturnRepositoryData() {
    	/* devo creare un record fittizio --> guardo le colonne dalla table "commands" nel 
    	 * sql file schema.sql contenuto in "src/main/resources" 
    	 * 
    	 * Oss.: alcuni campi saranno null */
//        List<Map<String, Object>> expected = List.of(
//                Map.of(
//                        "id", 1,
//                        "device_id", "DEV001",
//                        "type", "FORCE_MEASUREMENT",
//                        "payload", "immediate",
//                        "status", "PENDING",
//                        "created_at", "2026-07-13T09:14:18Z"
//                )
//        );
    	
    	/* record più semplice - dato MINIMALE, non serve il record completo */
        List<Map<String, Object>> expected = List.of(
                Map.of("id", 1, "type", "FORCE_MEASUREMENT", "status", "PENDING")
        );

        // stub del mock
        when(commandRepository.findCommandsByDeviceId("DEV001")).thenReturn(expected);

        // chiamata del service reale
        List<Map<String, Object>> result = commandService.getCommandsByDeviceId("DEV001");

        assertEquals(expected, result);
        verify(commandRepository, times(1)).findCommandsByDeviceId("DEV001");
    }
    
    
    @Test
    void getPendingCommandDtosByDeviceId_shouldReturnMappedDtos() {
        // arrange
        Map<String, Object> row = new HashMap<>();
        row.put("id", 1);
        row.put("device_id", "DEV001");
        row.put("type", "FORCE_MEASUREMENT");
        row.put("payload", "immediate");
        row.put("status", "PENDING");
        row.put("created_at", "2026-07-13T09:14:18Z");
        row.put("ack_at", null);
        row.put("result_message", null);

        List<Map<String, Object>> rows = List.of(row);

        when(commandRepository.findPendingCommandsByDeviceId("DEV001")).thenReturn(rows);

        // act
        List<PendingCommandResponse> actual = commandService.getPendingCommandDtosByDeviceId("DEV001");

        // assert
        assertEquals(1, actual.size());

        PendingCommandResponse dto = actual.get(0);
        assertEquals(1L, dto.getId());
        assertEquals("DEV001", dto.getDeviceId());
        assertEquals("FORCE_MEASUREMENT", dto.getType());
        assertEquals("immediate", dto.getPayload());
        assertEquals("PENDING", dto.getStatus());
        assertEquals("2026-07-13T09:14:18Z", dto.getCreatedAt());
        assertNull(dto.getAckAt());
        assertNull(dto.getResultMessage());

        verify(commandRepository, times(1)).findPendingCommandsByDeviceId("DEV001");
    }

    @Test
    void ackCommand_shouldDelegateToRepository() {
        CommandAckRequest request = new CommandAckRequest();
        request.setStatus("EXECUTED");
        request.setAckAt("2026-07-13T09:14:18Z");
        request.setResultMessage("Measurement executed successfully");

        commandService.ackCommand(1L, request);

        verify(commandRepository, times(1)).ackCommand(1L, request);
    }
}

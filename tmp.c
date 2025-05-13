import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.time.*;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
class ParisTimeToUtcDeserializerTest {

    private final ParisTimeToUtcDeserializer deserializer = new ParisTimeToUtcDeserializer();
    
    @Mock
    private JsonParser jsonParser;
    
    @Mock
    private DeserializationContext context;

    @Test
    void deserialize_parisSummerTime_convertsToUtc() throws Exception {
        when(jsonParser.getText()).thenReturn("2025-05-06T17:30:00");
        LocalDateTime result = deserializer.deserialize(jsonParser, context);
        assertEquals(LocalDateTime.of(2025, 5, 6, 15, 30), result);
    }

    @Test
    void deserialize_parisWinterTime_convertsToUtc() throws Exception {
        when(jsonParser.getText()).thenReturn("2025-01-06T17:30:00");
        LocalDateTime result = deserializer.deserialize(jsonParser, context);
        assertEquals(LocalDateTime.of(2025, 1, 6, 16, 30), result);
    }

    @Test
    void deserialize_midnightTime_convertsCorrectly() throws Exception {
        when(jsonParser.getText()).thenReturn("2025-05-06T00:00:00");
        LocalDateTime result = deserializer.deserialize(jsonParser, context);
        assertEquals(LocalDateTime.of(2025, 5, 5, 22, 0), result);
    }

    @Test
    void deserialize_withLeapSeconds_handlesCorrectly() throws Exception {
        when(jsonParser.getText()).thenReturn("2025-06-30T23:59:60");
        LocalDateTime result = deserializer.deserialize(jsonParser, context);
        assertEquals(LocalDateTime.of(2025, 6, 30, 21, 59, 59), result);
    }

    @Test
    void deserialize_invalidFormat_throwsException() {
        when(jsonParser.getText()).thenReturn("2025/05/06 17:30");
        assertThrows(DateTimeParseException.class, () -> {
            deserializer.deserialize(jsonParser, context);
        });
    }

    @Test
    void deserialize_nullInput_throwsException() {
        when(jsonParser.getText()).thenReturn(null);
        assertThrows(NullPointerException.class, () -> {
            deserializer.deserialize(jsonParser, context);
        });
    }

    @Test
    void deserialize_dstTransitionForward_convertsCorrectly() throws Exception {
        when(jsonParser.getText()).thenReturn("2025-03-30T02:30:00");
        LocalDateTime result = deserializer.deserialize(jsonParser, context);
        assertEquals(LocalDateTime.of(2025, 3, 30, 1, 30), result);
    }

    @Test
    void deserialize_dstTransitionBackward_convertsCorrectly() throws Exception {
        when(jsonParser.getText()).thenReturn("2025-10-26T02:30:00");
        LocalDateTime result = deserializer.deserialize(jsonParser, context);
        assertEquals(LocalDateTime.of(2025, 10, 26, 0, 30), result);
    }

    @ParameterizedTest
    @CsvSource({
        "2025-05-06T17:30:00, 2025-05-06T15:30:00",
        "2025-01-06T17:30:00, 2025-01-06T16:30:00",
        "2025-05-06T00:00:00, 2025-05-05T22:00:00",
        "2025-12-31T23:59:59, 2025-12-31T22:59:59"
    })
    void deserialize_variousTimes_convertsCorrectly(String input, String expected) throws Exception {
        when(jsonParser.getText()).thenReturn(input);
        LocalDateTime result = deserializer.deserialize(jsonParser, context);
        assertEquals(LocalDateTime.parse(expected), result);
    }

    @Test
    void deserialize_verifiesParserCalled() throws Exception {
        when(jsonParser.getText()).thenReturn("2025-05-06T17:30:00");
        deserializer.deserialize(jsonParser, context);
        verify(jsonParser, times(1)).getText();
    }
}

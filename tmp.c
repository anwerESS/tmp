import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.SerializerProvider;
import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.io.StringWriter;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.time.format.DateTimeFormatter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.mock;

class LocalDateTimeToUtcSerializerTest {

    @Test
    void testSerializeParisTimeToUtc() throws IOException {
        // Arrange
        LocalDateTimeToUtcSerializer serializer = new LocalDateTimeToUtcSerializer();
        // Paris time: 17:30 on March 25, 2024 (before daylight saving kicks in)
        LocalDateTime parisTime = LocalDateTime.of(2024, 3, 25, 17, 30);

        // Use a real StringWriter to capture the output
        StringWriter output = new StringWriter();
        JsonGenerator gen = new com.fasterxml.jackson.core.json.JsonFactory().createGenerator(output);
        SerializerProvider provider = mock(SerializerProvider.class);

        // Act
        serializer.serialize(parisTime, gen, provider);
        gen.close();

        // Extract the resulting UTC time string
        String result = output.toString().replace("\"", "");  // remove quotes from JSON string

        // Expected UTC time: Paris is UTC+1 in March (before DST), so 17:30 -> 16:30 UTC
        String expected = "2024-03-25T16:30:00Z";

        // Assert
        assertEquals(expected, result);
    }
}
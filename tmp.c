import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.SerializerProvider;
import org.junit.jupiter.api.Test;
import java.io.IOException;
import java.io.StringWriter;
import java.time.LocalDateTime;
import static org.mockito.Mockito.*;

class LocalDateTimeToUtcSerializerTest {

    @Test
    void testSerialize() throws IOException {
        LocalDateTimeToUtcSerializer serializer = new LocalDateTimeToUtcSerializer();
        LocalDateTime dateTime = LocalDateTime.of(2024, 5, 10, 12, 0);

        StringWriter writer = new StringWriter();
        JsonGenerator gen = mock(JsonGenerator.class);
        SerializerProvider provider = mock(SerializerProvider.class);

        serializer.serialize(dateTime, gen, provider);

        verify(gen, times(1)).writeString(anyString());
    }
}
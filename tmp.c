public class InvalidBookReferenceException extends RuntimeException {
    private final String bookReference;
    
    public InvalidBookReferenceException(String bookReference, String message) {
        super(message);
        this.bookReference = bookReference;
    }
    
    public String getBookReference() {
        return bookReference;
    }
}

========================================================================================
@Getter
@Builder
public class ErrorResponse {
    private final LocalDateTime timestamp;
    private final int status;
    private final String error;
    private final String message;
    private final String path;
    private final Map<String, Object> details;
    
    public static ErrorResponse of(HttpStatus status, String message, String path, Map<String, Object> details) {
        return ErrorResponse.builder()
            .timestamp(LocalDateTime.now())
            .status(status.value())
            .error(status.getReasonPhrase())
            .message(message)
            .path(path)
            .details(details)
            .build();
    }
}
========================================================================================
  @RestControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(InvalidBookReferenceException.class)
    public ResponseEntity<ErrorResponse> handleInvalidBookReference(InvalidBookReferenceException ex, 
                                                                  WebRequest request) {
        Map<String, Object> details = new HashMap<>();
        details.put("bookReference", ex.getBookReference());
        details.put("pattern", "Must be 1-16 alphanumeric characters");
        
        ErrorResponse errorResponse = ErrorResponse.of(
            HttpStatus.BAD_REQUEST,
            ex.getMessage(),
            request.getDescription(false),
            details
        );
        
        return ResponseEntity.badRequest().body(errorResponse);
    }
    
    // Add other exception handlers as needed
}
========================================================================================
public class BookReferenceValidator {
    private static final Pattern BOOK_REFERENCE_PATTERN = Pattern.compile("^[a-zA-Z0-9]{1,16}$");
    
    public static void validate(String bookReference) {
        if (bookReference == null || !BOOK_REFERENCE_PATTERN.matcher(bookReference).matches()) {
            throw new InvalidBookReferenceException(
                bookReference,
                "Book reference must be 1-16 alphanumeric characters"
            );
        }
    }
}
========================================================================================
@GetMapping("/forex/{bookReference}")
public ResponseEntity<ForexResponseDto> getForexByBookReference(
    @PathVariable("bookReference") String bookReference
) {
    // Validate input
    BookReferenceValidator.validate(bookReference);
    
    // Rest of your logic
    try {
        Forex forexResult = forexService.findForex(bookReference).join();
        return ResponseEntity.ok(
            ForexResponseDto.builder()
                .withDetails(forexDtoMapper.toDto(forexResult))
                .build()
        );
    } catch (CompletionException e) {
        // Handle other exceptions
    }
}

@Test
void getForexByBookReference_shouldThrowException_whenBookReferenceInvalid() {
    String invalidReference = "INVALID123";

    when(bookReferenceValidator.validate(invalidReference)).thenReturn(false);

    assertThrows(InvalidBookReferenceException.class, () -> {
        controller.getForexByBookReference(invalidReference);
    });
}

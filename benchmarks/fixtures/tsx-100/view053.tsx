interface Props53 { value: number; label: string }
export const View53 = (p: Props53) => (
  <section data-i="53">
    <h2>{p.label}</h2>
    {p.value > 53 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

interface Props57 { value: number; label: string }
export const View57 = (p: Props57) => (
  <section data-i="57">
    <h2>{p.label}</h2>
    {p.value > 57 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

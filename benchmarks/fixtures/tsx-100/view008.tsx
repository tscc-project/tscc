interface Props8 { value: number; label: string }
export const View8 = (p: Props8) => (
  <section data-i="8">
    <h2>{p.label}</h2>
    {p.value > 8 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

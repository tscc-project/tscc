interface Props42 { value: number; label: string }
export const View42 = (p: Props42) => (
  <section data-i="42">
    <h2>{p.label}</h2>
    {p.value > 42 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

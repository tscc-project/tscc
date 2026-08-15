interface Props79 { value: number; label: string }
export const View79 = (p: Props79) => (
  <section data-i="79">
    <h2>{p.label}</h2>
    {p.value > 79 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

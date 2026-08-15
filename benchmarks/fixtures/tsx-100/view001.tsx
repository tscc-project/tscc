interface Props1 { value: number; label: string }
export const View1 = (p: Props1) => (
  <section data-i="1">
    <h2>{p.label}</h2>
    {p.value > 1 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

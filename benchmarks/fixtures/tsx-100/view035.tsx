interface Props35 { value: number; label: string }
export const View35 = (p: Props35) => (
  <section data-i="35">
    <h2>{p.label}</h2>
    {p.value > 35 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

interface Props50 { value: number; label: string }
export const View50 = (p: Props50) => (
  <section data-i="50">
    <h2>{p.label}</h2>
    {p.value > 50 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

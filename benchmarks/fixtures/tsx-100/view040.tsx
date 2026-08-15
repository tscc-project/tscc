interface Props40 { value: number; label: string }
export const View40 = (p: Props40) => (
  <section data-i="40">
    <h2>{p.label}</h2>
    {p.value > 40 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

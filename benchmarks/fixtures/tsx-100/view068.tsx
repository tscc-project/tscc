interface Props68 { value: number; label: string }
export const View68 = (p: Props68) => (
  <section data-i="68">
    <h2>{p.label}</h2>
    {p.value > 68 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

interface Props20 { value: number; label: string }
export const View20 = (p: Props20) => (
  <section data-i="20">
    <h2>{p.label}</h2>
    {p.value > 20 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

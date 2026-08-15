interface Props0 { value: number; label: string }
export const View0 = (p: Props0) => (
  <section data-i="0">
    <h2>{p.label}</h2>
    {p.value > 0 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

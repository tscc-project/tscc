interface Props65 { value: number; label: string }
export const View65 = (p: Props65) => (
  <section data-i="65">
    <h2>{p.label}</h2>
    {p.value > 65 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

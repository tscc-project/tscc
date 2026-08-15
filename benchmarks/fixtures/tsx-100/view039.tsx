interface Props39 { value: number; label: string }
export const View39 = (p: Props39) => (
  <section data-i="39">
    <h2>{p.label}</h2>
    {p.value > 39 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

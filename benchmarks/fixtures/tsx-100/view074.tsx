interface Props74 { value: number; label: string }
export const View74 = (p: Props74) => (
  <section data-i="74">
    <h2>{p.label}</h2>
    {p.value > 74 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

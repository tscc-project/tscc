interface Props75 { value: number; label: string }
export const View75 = (p: Props75) => (
  <section data-i="75">
    <h2>{p.label}</h2>
    {p.value > 75 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

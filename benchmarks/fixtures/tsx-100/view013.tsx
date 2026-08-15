interface Props13 { value: number; label: string }
export const View13 = (p: Props13) => (
  <section data-i="13">
    <h2>{p.label}</h2>
    {p.value > 13 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

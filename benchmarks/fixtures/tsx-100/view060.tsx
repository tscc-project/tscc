interface Props60 { value: number; label: string }
export const View60 = (p: Props60) => (
  <section data-i="60">
    <h2>{p.label}</h2>
    {p.value > 60 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

interface Props54 { value: number; label: string }
export const View54 = (p: Props54) => (
  <section data-i="54">
    <h2>{p.label}</h2>
    {p.value > 54 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

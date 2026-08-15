interface Props84 { value: number; label: string }
export const View84 = (p: Props84) => (
  <section data-i="84">
    <h2>{p.label}</h2>
    {p.value > 84 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

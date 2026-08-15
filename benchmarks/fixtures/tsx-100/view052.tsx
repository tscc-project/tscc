interface Props52 { value: number; label: string }
export const View52 = (p: Props52) => (
  <section data-i="52">
    <h2>{p.label}</h2>
    {p.value > 52 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

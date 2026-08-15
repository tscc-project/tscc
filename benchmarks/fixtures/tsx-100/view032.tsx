interface Props32 { value: number; label: string }
export const View32 = (p: Props32) => (
  <section data-i="32">
    <h2>{p.label}</h2>
    {p.value > 32 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

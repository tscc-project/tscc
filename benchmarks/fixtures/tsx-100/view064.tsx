interface Props64 { value: number; label: string }
export const View64 = (p: Props64) => (
  <section data-i="64">
    <h2>{p.label}</h2>
    {p.value > 64 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);

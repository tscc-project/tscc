interface Props95 { value: number; label: string }
export const View95 = (p: Props95) => (
  <section data-i="95">
    <h2>{p.label}</h2>
    {p.value > 95 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
